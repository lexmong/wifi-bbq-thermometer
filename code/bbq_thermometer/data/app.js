var app = angular.module('app', ['ngRoute']);

app.config(function ($routeProvider, $locationProvider) {
    $routeProvider.when('/', {
        controller: 'tempCtrl',
        templateUrl: '/probes.html',
    });
});

app.config(function ($routeProvider, $locationProvider) {
    $routeProvider.when('/config', {
        controller: 'configCtrl',
        templateUrl: '/config.html',
    });
});

app.controller('tempCtrl', ['$scope', '$http','$interval', function ($scope, $http, $interval) {
    $scope.unit = 'c';

    $scope.probes = [
        {id:1,target:'-',temperature:-1},
        {id:2,target:'-',temperature:-1},
        {id:3,target:'-',temperature:-1},
        {id:4,target:'-',temperature:-1}
    ]

    function getTemp() {
        $http.get('/readTemperature',{
            params: {unit: $scope.unit}
        }).then((response) => {
            console.log(response);
            for(let i =0;i<4;i++){
                $scope.probes[i].temperature = Math.round(response.data[i]);
            }
        }, (response) => {
            console.log(response);
            for(let i =0;i<4;i++){
                $scope.probes[i].temperature =-1;
            }
        });
    }

    update = $interval(function() {
        getTemp();
    }, 2000);

    $scope.$on('$destroy', function() {
        $interval.cancel(update);
    });
}]);

app.controller('configCtrl', ['$scope', '$http', function ($scope, $http) {
    $scope.read = (i) => {
        $http.get('/readResistance',{
            params: {probe: $scope.probe}
        }).then((response) => {
            $scope.points[i].resistance = response.data[0];
        }, (response) => {
            console.log(response);
        });
    };

    $scope.change = () => {
        $scope.points = [
            {temperature:null,resistance:null},
            {temperature:null,resistance:null},
            {temperature:null,resistance:null},
        ];

        $scope.coeffs = [];
    };

    $scope.change();

    $scope.submit = () => {
        $http.get('/saveCoeffs',{
            params: {
                points: JSON.stringify($scope.points),
                probe: $scope.probe
            }
        }).then((response) => {
            console.log(response);
            $scope.coeffs = [
                response.data.a,
                response.data.b,
                response.data.c
            ];
        }, (response) => {
            console.log(response);
        });
    };

    $scope.toAlpha = (i) => String.fromCharCode(i+65);

}]);
